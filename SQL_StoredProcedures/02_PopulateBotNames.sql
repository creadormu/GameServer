USE [MuOnline]
GO

-- =============================================
-- Populate Bot Names (10 Languages × 2 Genders × 50+ Names)
-- =============================================

DELETE FROM [dbo].[BotNames]
GO

-- ENGLISH MALE NAMES (50+)
INSERT INTO [dbo].[BotNames] ([Name], [Language], [Gender]) VALUES
('Alexander', 'English', 'Male'), ('Benjamin', 'English', 'Male'), ('Christopher', 'English', 'Male'),
('Daniel', 'English', 'Male'), ('Edward', 'English', 'Male'), ('Frederick', 'English', 'Male'),
('Gabriel', 'English', 'Male'), ('Harrison', 'English', 'Male'), ('Isaac', 'English', 'Male'),
('Jacob', 'English', 'Male'), ('Kenneth', 'English', 'Male'), ('Lawrence', 'English', 'Male'),
('Michael', 'English', 'Male'), ('Nicholas', 'English', 'Male'), ('Oliver', 'English', 'Male'),
('Patrick', 'English', 'Male'), ('Quincy', 'English', 'Male'), ('Richard', 'English', 'Male'),
('Samuel', 'English', 'Male'), ('Theodore', 'English', 'Male'), ('Victor', 'English', 'Male'),
('William', 'English', 'Male'), ('Xavier', 'English', 'Male'), ('Zachary', 'English', 'Male'),
('Adrian', 'English', 'Male'), ('Brandon', 'English', 'Male'), ('Cameron', 'English', 'Male'),
('Dominic', 'English', 'Male'), ('Ethan', 'English', 'Male'), ('Felix', 'English', 'Male'),
('Gregory', 'English', 'Male'), ('Henry', 'English', 'Male'), ('Ivan', 'English', 'Male'),
('Julian', 'English', 'Male'), ('Kevin', 'English', 'Male'), ('Lucas', 'English', 'Male'),
('Marcus', 'English', 'Male'), ('Nathan', 'English', 'Male'), ('Oscar', 'English', 'Male'),
('Peter', 'English', 'Male'), ('Raymond', 'English', 'Male'), ('Sebastian', 'English', 'Male'),
('Thomas', 'English', 'Male'), ('Vincent', 'English', 'Male'), ('Wesley', 'English', 'Male'),
('Arthur', 'English', 'Male'), ('Bradley', 'English', 'Male'), ('Calvin', 'English', 'Male'),
('Derek', 'English', 'Male'), ('Edgar', 'English', 'Male'), ('Francis', 'English', 'Male'),
('George', 'English', 'Male'), ('Harold', 'English', 'Male'), ('Irving', 'English', 'Male'),
('Jason', 'English', 'Male'), ('Keith', 'English', 'Male')

-- ENGLISH FEMALE NAMES (50+)
INSERT INTO [dbo].[BotNames] ([Name], [Language], [Gender]) VALUES
('Abigail', 'English', 'Female'), ('Beatrice', 'English', 'Female'), ('Catherine', 'English', 'Female'),
('Diana', 'English', 'Female'), ('Eleanor', 'English', 'Female'), ('Fiona', 'English', 'Female'),
('Grace', 'English', 'Female'), ('Hannah', 'English', 'Female'), ('Isabella', 'English', 'Female'),
('Jessica', 'English', 'Female'), ('Katherine', 'English', 'Female'), ('Lillian', 'English', 'Female'),
('Margaret', 'English', 'Female'), ('Natalie', 'English', 'Female'), ('Olivia', 'English', 'Female'),
('Penelope', 'English', 'Female'), ('Rebecca', 'English', 'Female'), ('Sophia', 'English', 'Female'),
('Victoria', 'English', 'Female'), ('Willow', 'English', 'Female'), ('Alexandra', 'English', 'Female'),
('Bridget', 'English', 'Female'), ('Caroline', 'English', 'Female'), ('Danielle', 'English', 'Female'),
('Elizabeth', 'English', 'Female'), ('Francesca', 'English', 'Female'), ('Gabrielle', 'English', 'Female'),
('Helena', 'English', 'Female'), ('Iris', 'English', 'Female'), ('Julia', 'English', 'Female'),
('Kimberly', 'English', 'Female'), ('Louise', 'English', 'Female'), ('Miranda', 'English', 'Female'),
('Nicole', 'English', 'Female'), ('Ophelia', 'English', 'Female'), ('Priscilla', 'English', 'Female'),
('Rachel', 'English', 'Female'), ('Samantha', 'English', 'Female'), ('Teresa', 'English', 'Female'),
('Valerie', 'English', 'Female'), ('Wendy', 'English', 'Female'), ('Yvonne', 'English', 'Female'),
('Adriana', 'English', 'Female'), ('Bethany', 'English', 'Female'), ('Cassandra', 'English', 'Female'),
('Deborah', 'English', 'Female'), ('Emily', 'English', 'Female'), ('Florence', 'English', 'Female'),
('Genevieve', 'English', 'Female'), ('Hazel', 'English', 'Female'), ('Irene', 'English', 'Female'),
('Jennifer', 'English', 'Female'), ('Kristina', 'English', 'Female'), ('Laura', 'English', 'Female'),
('Michelle', 'English', 'Female'), ('Nancy', 'English', 'Female')

-- SPANISH MALE NAMES (50+)
INSERT INTO [dbo].[BotNames] ([Name], [Language], [Gender]) VALUES
('Alejandro', 'Spanish', 'Male'), ('Benito', 'Spanish', 'Male'), ('Carlos', 'Spanish', 'Male'),
('Diego', 'Spanish', 'Male'), ('Eduardo', 'Spanish', 'Male'), ('Fernando', 'Spanish', 'Male'),
('Gonzalo', 'Spanish', 'Male'), ('Hector', 'Spanish', 'Male'), ('Ignacio', 'Spanish', 'Male'),
('Javier', 'Spanish', 'Male'), ('Lorenzo', 'Spanish', 'Male'), ('Manuel', 'Spanish', 'Male'),
('Nicolas', 'Spanish', 'Male'), ('Osvaldo', 'Spanish', 'Male'), ('Pablo', 'Spanish', 'Male'),
('Rafael', 'Spanish', 'Male'), ('Santiago', 'Spanish', 'Male'), ('Tomas', 'Spanish', 'Male'),
('Vicente', 'Spanish', 'Male'), ('Alberto', 'Spanish', 'Male'), ('Bernardo', 'Spanish', 'Male'),
('Cristian', 'Spanish', 'Male'), ('Dario', 'Spanish', 'Male'), ('Emilio', 'Spanish', 'Male'),
('Felipe', 'Spanish', 'Male'), ('Guillermo', 'Spanish', 'Male'), ('Horacio', 'Spanish', 'Male'),
('Ismael', 'Spanish', 'Male'), ('Jorge', 'Spanish', 'Male'), ('Luis', 'Spanish', 'Male'),
('Marcos', 'Spanish', 'Male'), ('Nestor', 'Spanish', 'Male'), ('Omar', 'Spanish', 'Male'),
('Pedro', 'Spanish', 'Male'), ('Ramiro', 'Spanish', 'Male'), ('Sergio', 'Spanish', 'Male'),
('Teodoro', 'Spanish', 'Male'), ('Valentin', 'Spanish', 'Male'), ('Andres', 'Spanish', 'Male'),
('Bruno', 'Spanish', 'Male'), ('Cesar', 'Spanish', 'Male'), ('Damian', 'Spanish', 'Male'),
('Enrique', 'Spanish', 'Male'), ('Francisco', 'Spanish', 'Male'), ('Gabriel', 'Spanish', 'Male'),
('Hugo', 'Spanish', 'Male'), ('Ivan', 'Spanish', 'Male'), ('Julio', 'Spanish', 'Male'),
('Leonardo', 'Spanish', 'Male'), ('Martin', 'Spanish', 'Male'), ('Nataniel', 'Spanish', 'Male'),
('Oscar', 'Spanish', 'Male'), ('Rodrigo', 'Spanish', 'Male'), ('Salvador', 'Spanish', 'Male'),
('Tiago', 'Spanish', 'Male'), ('Victor', 'Spanish', 'Male')

-- SPANISH FEMALE NAMES (50+)
INSERT INTO [dbo].[BotNames] ([Name], [Language], [Gender]) VALUES
('Adriana', 'Spanish', 'Female'), ('Beatriz', 'Spanish', 'Female'), ('Carmen', 'Spanish', 'Female'),
('Daniela', 'Spanish', 'Female'), ('Elena', 'Spanish', 'Female'), ('Fernanda', 'Spanish', 'Female'),
('Gloria', 'Spanish', 'Female'), ('Helena', 'Spanish', 'Female'), ('Isabel', 'Spanish', 'Female'),
('Jazmin', 'Spanish', 'Female'), ('Laura', 'Spanish', 'Female'), ('Monica', 'Spanish', 'Female'),
('Natalia', 'Spanish', 'Female'), ('Olivia', 'Spanish', 'Female'), ('Patricia', 'Spanish', 'Female'),
('Raquel', 'Spanish', 'Female'), ('Sofia', 'Spanish', 'Female'), ('Teresa', 'Spanish', 'Female'),
('Valentina', 'Spanish', 'Female'), ('Andrea', 'Spanish', 'Female'), ('Bianca', 'Spanish', 'Female'),
('Camila', 'Spanish', 'Female'), ('Diana', 'Spanish', 'Female'), ('Emilia', 'Spanish', 'Female'),
('Francisca', 'Spanish', 'Female'), ('Gabriela', 'Spanish', 'Female'), ('Ines', 'Spanish', 'Female'),
('Josefina', 'Spanish', 'Female'), ('Lucia', 'Spanish', 'Female'), ('Maria', 'Spanish', 'Female'),
('Nadia', 'Spanish', 'Female'), ('Paloma', 'Spanish', 'Female'), ('Rosa', 'Spanish', 'Female'),
('Sara', 'Spanish', 'Female'), ('Tatiana', 'Spanish', 'Female'), ('Veronica', 'Spanish', 'Female'),
('Alejandra', 'Spanish', 'Female'), ('Belen', 'Spanish', 'Female'), ('Carolina', 'Spanish', 'Female'),
('Dolores', 'Spanish', 'Female'), ('Esther', 'Spanish', 'Female'), ('Fatima', 'Spanish', 'Female'),
('Graciela', 'Spanish', 'Female'), ('Irene', 'Spanish', 'Female'), ('Jimena', 'Spanish', 'Female'),
('Lorena', 'Spanish', 'Female'), ('Mariana', 'Spanish', 'Female'), ('Noelia', 'Spanish', 'Female'),
('Paola', 'Spanish', 'Female'), ('Rocio', 'Spanish', 'Female'), ('Silvia', 'Spanish', 'Female'),
('Tamara', 'Spanish', 'Female'), ('Victoria', 'Spanish', 'Female'), ('Yolanda', 'Spanish', 'Female'),
('Alicia', 'Spanish', 'Female'), ('Blanca', 'Spanish', 'Female')

-- PORTUGUESE MALE NAMES (50+)
INSERT INTO [dbo].[BotNames] ([Name], [Language], [Gender]) VALUES
('Andre', 'Portuguese', 'Male'), ('Bruno', 'Portuguese', 'Male'), ('Carlos', 'Portuguese', 'Male'),
('Daniel', 'Portuguese', 'Male'), ('Eduardo', 'Portuguese', 'Male'), ('Fernando', 'Portuguese', 'Male'),
('Gustavo', 'Portuguese', 'Male'), ('Henrique', 'Portuguese', 'Male'), ('Igor', 'Portuguese', 'Male'),
('Joao', 'Portuguese', 'Male'), ('Leonardo', 'Portuguese', 'Male'), ('Marcelo', 'Portuguese', 'Male'),
('Nuno', 'Portuguese', 'Male'), ('Otavio', 'Portuguese', 'Male'), ('Paulo', 'Portuguese', 'Male'),
('Rafael', 'Portuguese', 'Male'), ('Sergio', 'Portuguese', 'Male'), ('Tiago', 'Portuguese', 'Male'),
('Vinicius', 'Portuguese', 'Male'), ('Alexandre', 'Portuguese', 'Male'), ('Bernardo', 'Portuguese', 'Male'),
('Caio', 'Portuguese', 'Male'), ('Diego', 'Portuguese', 'Male'), ('Erico', 'Portuguese', 'Male'),
('Fabio', 'Portuguese', 'Male'), ('Gabriel', 'Portuguese', 'Male'), ('Heitor', 'Portuguese', 'Male'),
('Ivan', 'Portuguese', 'Male'), ('Julio', 'Portuguese', 'Male'), ('Lucas', 'Portuguese', 'Male'),
('Mateus', 'Portuguese', 'Male'), ('Nicolas', 'Portuguese', 'Male'), ('Pedro', 'Portuguese', 'Male'),
('Ricardo', 'Portuguese', 'Male'), ('Samuel', 'Portuguese', 'Male'), ('Thiago', 'Portuguese', 'Male'),
('Victor', 'Portuguese', 'Male'), ('Antonio', 'Portuguese', 'Male'), ('Benjamin', 'Portuguese', 'Male'),
('Cesar', 'Portuguese', 'Male'), ('Davi', 'Portuguese', 'Male'), ('Emanuel', 'Portuguese', 'Male'),
('Felipe', 'Portuguese', 'Male'), ('Guilherme', 'Portuguese', 'Male'), ('Hugo', 'Portuguese', 'Male'),
('Isaque', 'Portuguese', 'Male'), ('Jose', 'Portuguese', 'Male'), ('Leandro', 'Portuguese', 'Male'),
('Miguel', 'Portuguese', 'Male'), ('Nathan', 'Portuguese', 'Male'), ('Orlando', 'Portuguese', 'Male'),
('Renato', 'Portuguese', 'Male'), ('Rodrigo', 'Portuguese', 'Male'), ('Teodoro', 'Portuguese', 'Male'),
('Vicente', 'Portuguese', 'Male'), ('Yuri', 'Portuguese', 'Male')

-- PORTUGUESE FEMALE NAMES (50+)
INSERT INTO [dbo].[BotNames] ([Name], [Language], [Gender]) VALUES
('Ana', 'Portuguese', 'Female'), ('Beatriz', 'Portuguese', 'Female'), ('Camila', 'Portuguese', 'Female'),
('Daniela', 'Portuguese', 'Female'), ('Elena', 'Portuguese', 'Female'), ('Fernanda', 'Portuguese', 'Female'),
('Gabriela', 'Portuguese', 'Female'), ('Helena', 'Portuguese', 'Female'), ('Isabela', 'Portuguese', 'Female'),
('Julia', 'Portuguese', 'Female'), ('Larissa', 'Portuguese', 'Female'), ('Marina', 'Portuguese', 'Female'),
('Natalia', 'Portuguese', 'Female'), ('Olivia', 'Portuguese', 'Female'), ('Paula', 'Portuguese', 'Female'),
('Rafaela', 'Portuguese', 'Female'), ('Sara', 'Portuguese', 'Female'), ('Tatiana', 'Portuguese', 'Female'),
('Valentina', 'Portuguese', 'Female'), ('Alice', 'Portuguese', 'Female'), ('Bianca', 'Portuguese', 'Female'),
('Carolina', 'Portuguese', 'Female'), ('Diana', 'Portuguese', 'Female'), ('Eduarda', 'Portuguese', 'Female'),
('Francisca', 'Portuguese', 'Female'), ('Giovana', 'Portuguese', 'Female'), ('Heloisa', 'Portuguese', 'Female'),
('Ingrid', 'Portuguese', 'Female'), ('Joana', 'Portuguese', 'Female'), ('Leticia', 'Portuguese', 'Female'),
('Mariana', 'Portuguese', 'Female'), ('Nicole', 'Portuguese', 'Female'), ('Patricia', 'Portuguese', 'Female'),
('Renata', 'Portuguese', 'Female'), ('Sofia', 'Portuguese', 'Female'), ('Thais', 'Portuguese', 'Female'),
('Vanessa', 'Portuguese', 'Female'), ('Amanda', 'Portuguese', 'Female'), ('Bruna', 'Portuguese', 'Female'),
('Catarina', 'Portuguese', 'Female'), ('Debora', 'Portuguese', 'Female'), ('Elisa', 'Portuguese', 'Female'),
('Fabiana', 'Portuguese', 'Female'), ('Gisele', 'Portuguese', 'Female'), ('Ines', 'Portuguese', 'Female'),
('Juliana', 'Portuguese', 'Female'), ('Luana', 'Portuguese', 'Female'), ('Melissa', 'Portuguese', 'Female'),
('Nara', 'Portuguese', 'Female'), ('Priscila', 'Portuguese', 'Female'), ('Roberta', 'Portuguese', 'Female'),
('Sabrina', 'Portuguese', 'Female'), ('Teresa', 'Portuguese', 'Female'), ('Vitoria', 'Portuguese', 'Female'),
('Yasmin', 'Portuguese', 'Female'), ('Adriana', 'Portuguese', 'Female')

-- KOREAN MALE NAMES (50+)
INSERT INTO [dbo].[BotNames] ([Name], [Language], [Gender]) VALUES
('MinHo', 'Korean', 'Male'), ('JinWoo', 'Korean', 'Male'), ('TaeYang', 'Korean', 'Male'),
('SeungHo', 'Korean', 'Male'), ('DongHyun', 'Korean', 'Male'), ('KyungSoo', 'Korean', 'Male'),
('JunSeo', 'Korean', 'Male'), ('WooJin', 'Korean', 'Male'), ('HyunWoo', 'Korean', 'Male'),
('SangHoon', 'Korean', 'Male'), ('JaeMin', 'Korean', 'Male'), ('YoungJae', 'Korean', 'Male'),
('GiSung', 'Korean', 'Male'), ('DaeHyun', 'Korean', 'Male'), ('MyungHo', 'Korean', 'Male'),
('ChangMin', 'Korean', 'Male'), ('JiHoon', 'Korean', 'Male'), ('SeoJun', 'Korean', 'Male'),
('HanGyul', 'Korean', 'Male'), ('MinSeok', 'Korean', 'Male'), ('JongIn', 'Korean', 'Male'),
('TaeHyung', 'Korean', 'Male'), ('YoonGi', 'Korean', 'Male'), ('NamJoon', 'Korean', 'Male'),
('HoSeok', 'Korean', 'Male'), ('JungKook', 'Korean', 'Male'), ('SangWoo', 'Korean', 'Male'),
('ByungHo', 'Korean', 'Male'), ('InSung', 'Korean', 'Male'), ('HaeJoon', 'Korean', 'Male'),
('SiWoo', 'Korean', 'Male'), ('EunWoo', 'Korean', 'Male'), ('JaeHyun', 'Korean', 'Male'),
('WonSik', 'Korean', 'Male'), ('KwangSoo', 'Korean', 'Male'), ('JiWon', 'Korean', 'Male'),
('SungMin', 'Korean', 'Male'), ('DongWook', 'Korean', 'Male'), ('YeongHo', 'Korean', 'Male'),
('GeonWoo', 'Korean', 'Male'), ('HyukJin', 'Korean', 'Male'), ('SuHo', 'Korean', 'Male'),
('BaekHyun', 'Korean', 'Male'), ('ChanYeol', 'Korean', 'Male'), ('SangYoon', 'Korean', 'Male'),
('JungMin', 'Korean', 'Male'), ('TaeKwon', 'Korean', 'Male'), ('MinSu', 'Korean', 'Male'),
('JoonHo', 'Korean', 'Male'), ('SeokJin', 'Korean', 'Male'), ('YongSu', 'Korean', 'Male'),
('HyunJae', 'Korean', 'Male'), ('JinHo', 'Korean', 'Male'), ('MinGyu', 'Korean', 'Male'),
('SungWoo', 'Korean', 'Male'), ('TaeMin', 'Korean', 'Male')

-- KOREAN FEMALE NAMES (50+)
INSERT INTO [dbo].[BotNames] ([Name], [Language], [Gender]) VALUES
('MinJi', 'Korean', 'Female'), ('SoYeon', 'Korean', 'Female'), ('JiHye', 'Korean', 'Female'),
('YuNa', 'Korean', 'Female'), ('HyeJin', 'Korean', 'Female'), ('SeoHyun', 'Korean', 'Female'),
('EunJi', 'Korean', 'Female'), ('NaYeon', 'Korean', 'Female'), ('DaHyun', 'Korean', 'Female'),
('ChaeYoung', 'Korean', 'Female'), ('JiSoo', 'Korean', 'Female'), ('SuJin', 'Korean', 'Female'),
('YeJi', 'Korean', 'Female'), ('RyuJin', 'Korean', 'Female'), ('YuJin', 'Korean', 'Female'),
('MiYoung', 'Korean', 'Female'), ('HaNeul', 'Korean', 'Female'), ('SeoJin', 'Korean', 'Female'),
('JiWon', 'Korean', 'Female'), ('SooMin', 'Korean', 'Female'), ('BoRa', 'Korean', 'Female'),
('HyoJung', 'Korean', 'Female'), ('SunMi', 'Korean', 'Female'), ('TaeYeon', 'Korean', 'Female'),
('YoRi', 'Korean', 'Female'), ('Tiffany', 'Korean', 'Female'), ('HyoYeon', 'Korean', 'Female'),
('SooYoung', 'Korean', 'Female'), ('YoonA', 'Korean', 'Female'), ('SeulGi', 'Korean', 'Female'),
('Irene', 'Korean', 'Female'), ('Wendy', 'Korean', 'Female'), ('Joy', 'Korean', 'Female'),
('Yeri', 'Korean', 'Female'), ('JeongYeon', 'Korean', 'Female'), ('Momo', 'Korean', 'Female'),
('Sana', 'Korean', 'Female'), ('MiNa', 'Korean', 'Female'), ('Tzuyu', 'Korean', 'Female'),
('Lia', 'Korean', 'Female'), ('ChaeRyeong', 'Korean', 'Female'), ('YuNa', 'Korean', 'Female'),
('SooA', 'Korean', 'Female'), ('MiYeon', 'Korean', 'Female'), ('MinNie', 'Korean', 'Female'),
('SoYeon', 'Korean', 'Female'), ('YuQi', 'Korean', 'Female'), ('ShuHua', 'Korean', 'Female'),
('EunBi', 'Korean', 'Female'), ('SakUra', 'Korean', 'Female'), ('HyeWon', 'Korean', 'Female'),
('YeNa', 'Korean', 'Female'), ('ChaeWon', 'Korean', 'Female'), ('MinJoo', 'Korean', 'Female'),
('NaKo', 'Korean', 'Female'), ('HiTomi', 'Korean', 'Female')

-- CHINESE MALE NAMES (50+)
INSERT INTO [dbo].[BotNames] ([Name], [Language], [Gender]) VALUES
('Wei', 'Chinese', 'Male'), ('Ming', 'Chinese', 'Male'), ('Jun', 'Chinese', 'Male'),
('Liang', 'Chinese', 'Male'), ('Feng', 'Chinese', 'Male'), ('Hao', 'Chinese', 'Male'),
('Cheng', 'Chinese', 'Male'), ('Jie', 'Chinese', 'Male'), ('Xing', 'Chinese', 'Male'),
('Yang', 'Chinese', 'Male'), ('Long', 'Chinese', 'Male'), ('Qiang', 'Chinese', 'Male'),
('Chen', 'Chinese', 'Male'), ('Bo', 'Chinese', 'Male'), ('Yong', 'Chinese', 'Male'),
('Tao', 'Chinese', 'Male'), ('Jiang', 'Chinese', 'Male'), ('Hui', 'Chinese', 'Male'),
('Kun', 'Chinese', 'Male'), ('Yuan', 'Chinese', 'Male'), ('Zhen', 'Chinese', 'Male'),
('Gang', 'Chinese', 'Male'), ('Peng', 'Chinese', 'Male'), ('Han', 'Chinese', 'Male'),
('Dong', 'Chinese', 'Male'), ('Zhong', 'Chinese', 'Male'), ('Lin', 'Chinese', 'Male'),
('Bin', 'Chinese', 'Male'), ('Xiao', 'Chinese', 'Male'), ('Sheng', 'Chinese', 'Male'),
('Yu', 'Chinese', 'Male'), ('Jian', 'Chinese', 'Male'), ('Lei', 'Chinese', 'Male'),
('Kai', 'Chinese', 'Male'), ('Rui', 'Chinese', 'Male'), ('Wen', 'Chinese', 'Male'),
('Hong', 'Chinese', 'Male'), ('Ping', 'Chinese', 'Male'), ('Hai', 'Chinese', 'Male'),
('An', 'Chinese', 'Male'), ('Guo', 'Chinese', 'Male'), ('Qing', 'Chinese', 'Male'),
('Shen', 'Chinese', 'Male'), ('Zhi', 'Chinese', 'Male'), ('Bao', 'Chinese', 'Male'),
('Guang', 'Chinese', 'Male'), ('Xuan', 'Chinese', 'Male'), ('Jin', 'Chinese', 'Male'),
('Shan', 'Chinese', 'Male'), ('Yun', 'Chinese', 'Male'), ('Ze', 'Chinese', 'Male'),
('Chao', 'Chinese', 'Male'), ('Da', 'Chinese', 'Male'), ('Ren', 'Chinese', 'Male'),
('Shi', 'Chinese', 'Male'), ('Tai', 'Chinese', 'Male')

-- CHINESE FEMALE NAMES (50+)
INSERT INTO [dbo].[BotNames] ([Name], [Language], [Gender]) VALUES
('Li', 'Chinese', 'Female'), ('Mei', 'Chinese', 'Female'), ('Ling', 'Chinese', 'Female'),
('Yan', 'Chinese', 'Female'), ('Xiu', 'Chinese', 'Female'), ('Hua', 'Chinese', 'Female'),
('Min', 'Chinese', 'Female'), ('Jing', 'Chinese', 'Female'), ('Fang', 'Chinese', 'Female'),
('Yue', 'Chinese', 'Female'), ('Lan', 'Chinese', 'Female'), ('Qian', 'Chinese', 'Female'),
('Xia', 'Chinese', 'Female'), ('Ying', 'Chinese', 'Female'), ('Hong', 'Chinese', 'Female'),
('Juan', 'Chinese', 'Female'), ('Shu', 'Chinese', 'Female'), ('Yun', 'Chinese', 'Female'),
('Rong', 'Chinese', 'Female'), ('Na', 'Chinese', 'Female'), ('Ping', 'Chinese', 'Female'),
('Xin', 'Chinese', 'Female'), ('Yu', 'Chinese', 'Female'), ('Hui', 'Chinese', 'Female'),
('Wen', 'Chinese', 'Female'), ('Jie', 'Chinese', 'Female'), ('Qing', 'Chinese', 'Female'),
('Yi', 'Chinese', 'Female'), ('Xuan', 'Chinese', 'Female'), ('Lei', 'Chinese', 'Female'),
('Ning', 'Chinese', 'Female'), ('Dan', 'Chinese', 'Female'), ('Chun', 'Chinese', 'Female'),
('Zhi', 'Chinese', 'Female'), ('Cui', 'Chinese', 'Female'), ('Fen', 'Chinese', 'Female'),
('Gui', 'Chinese', 'Female'), ('He', 'Chinese', 'Female'), ('Jin', 'Chinese', 'Female'),
('Kun', 'Chinese', 'Female'), ('Lian', 'Chinese', 'Female'), ('Miao', 'Chinese', 'Female'),
('Nuo', 'Chinese', 'Female'), ('Pei', 'Chinese', 'Female'), ('Qi', 'Chinese', 'Female'),
('Ru', 'Chinese', 'Female'), ('Shan', 'Chinese', 'Female'), ('Tao', 'Chinese', 'Female'),
('Wei', 'Chinese', 'Female'), ('Xiang', 'Chinese', 'Female'), ('Yan', 'Chinese', 'Female'),
('Zhen', 'Chinese', 'Female'), ('An', 'Chinese', 'Female'), ('Bao', 'Chinese', 'Female'),
('Chen', 'Chinese', 'Female'), ('Dong', 'Chinese', 'Female')

-- JAPANESE MALE NAMES (50+)
INSERT INTO [dbo].[BotNames] ([Name], [Language], [Gender]) VALUES
('Hiroshi', 'Japanese', 'Male'), ('Takeshi', 'Japanese', 'Male'), ('Kenji', 'Japanese', 'Male'),
('Yuki', 'Japanese', 'Male'), ('Daiki', 'Japanese', 'Male'), ('Haruto', 'Japanese', 'Male'),
('Sota', 'Japanese', 'Male'), ('Riku', 'Japanese', 'Male'), ('Kaito', 'Japanese', 'Male'),
('Yuuto', 'Japanese', 'Male'), ('Ryota', 'Japanese', 'Male'), ('Sho', 'Japanese', 'Male'),
('Akira', 'Japanese', 'Male'), ('Makoto', 'Japanese', 'Male'), ('Tatsuya', 'Japanese', 'Male'),
('Koji', 'Japanese', 'Male'), ('Satoshi', 'Japanese', 'Male'), ('Masato', 'Japanese', 'Male'),
('Kazuki', 'Japanese', 'Male'), ('Naoki', 'Japanese', 'Male'), ('Takumi', 'Japanese', 'Male'),
('Shota', 'Japanese', 'Male'), ('Kenta', 'Japanese', 'Male'), ('Tsubasa', 'Japanese', 'Male'),
('Hayato', 'Japanese', 'Male'), ('Ryusei', 'Japanese', 'Male'), ('Taiga', 'Japanese', 'Male'),
('Yuto', 'Japanese', 'Male'), ('Ren', 'Japanese', 'Male'), ('Hinata', 'Japanese', 'Male'),
('Sosuke', 'Japanese', 'Male'), ('Ryo', 'Japanese', 'Male'), ('Shinji', 'Japanese', 'Male'),
('Masashi', 'Japanese', 'Male'), ('Toshiro', 'Japanese', 'Male'), ('Minoru', 'Japanese', 'Male'),
('Isamu', 'Japanese', 'Male'), ('Hideo', 'Japanese', 'Male'), ('Osamu', 'Japanese', 'Male'),
('Noboru', 'Japanese', 'Male'), ('Ichiro', 'Japanese', 'Male'), ('Jiro', 'Japanese', 'Male'),
('Saburo', 'Japanese', 'Male'), ('Shiro', 'Japanese', 'Male'), ('Goro', 'Japanese', 'Male'),
('Rokuro', 'Japanese', 'Male'), ('Hachiro', 'Japanese', 'Male'), ('Kuro', 'Japanese', 'Male'),
('Taro', 'Japanese', 'Male'), ('Yukio', 'Japanese', 'Male'), ('Fumio', 'Japanese', 'Male'),
('Haruki', 'Japanese', 'Male'), ('Takuya', 'Japanese', 'Male'), ('Daisuke', 'Japanese', 'Male'),
('Hideaki', 'Japanese', 'Male'), ('Yasuhiro', 'Japanese', 'Male')

-- JAPANESE FEMALE NAMES (50+)
INSERT INTO [dbo].[BotNames] ([Name], [Language], [Gender]) VALUES
('Sakura', 'Japanese', 'Female'), ('Yui', 'Japanese', 'Female'), ('Hina', 'Japanese', 'Female'),
('Aoi', 'Japanese', 'Female'), ('Miyu', 'Japanese', 'Female'), ('Rin', 'Japanese', 'Female'),
('Mio', 'Japanese', 'Female'), ('Akari', 'Japanese', 'Female'), ('Yuna', 'Japanese', 'Female'),
('Kanna', 'Japanese', 'Female'), ('Nanami', 'Japanese', 'Female'), ('Himari', 'Japanese', 'Female'),
('Koharu', 'Japanese', 'Female'), ('Riko', 'Japanese', 'Female'), ('Saki', 'Japanese', 'Female'),
('Mei', 'Japanese', 'Female'), ('Natsuki', 'Japanese', 'Female'), ('Ayaka', 'Japanese', 'Female'),
('Misaki', 'Japanese', 'Female'), ('Hinata', 'Japanese', 'Female'), ('Rina', 'Japanese', 'Female'),
('Kaori', 'Japanese', 'Female'), ('Yuki', 'Japanese', 'Female'), ('Tomoko', 'Japanese', 'Female'),
('Noriko', 'Japanese', 'Female'), ('Ayumi', 'Japanese', 'Female'), ('Keiko', 'Japanese', 'Female'),
('Yoko', 'Japanese', 'Female'), ('Emi', 'Japanese', 'Female'), ('Kumiko', 'Japanese', 'Female'),
('Naomi', 'Japanese', 'Female'), ('Mariko', 'Japanese', 'Female'), ('Asuka', 'Japanese', 'Female'),
('Chiyo', 'Japanese', 'Female'), ('Haruka', 'Japanese', 'Female'), ('Kiyomi', 'Japanese', 'Female'),
('Midori', 'Japanese', 'Female'), ('Nana', 'Japanese', 'Female'), ('Rei', 'Japanese', 'Female'),
('Shiori', 'Japanese', 'Female'), ('Sumiko', 'Japanese', 'Female'), ('Takara', 'Japanese', 'Female'),
('Wakana', 'Japanese', 'Female'), ('Yumiko', 'Japanese', 'Female'), ('Akane', 'Japanese', 'Female'),
('Chika', 'Japanese', 'Female'), ('Etsuko', 'Japanese', 'Female'), ('Hanako', 'Japanese', 'Female'),
('Izumi', 'Japanese', 'Female'), ('Junko', 'Japanese', 'Female'), ('Kazuko', 'Japanese', 'Female'),
('Maki', 'Japanese', 'Female'), ('Natsumi', 'Japanese', 'Female'), ('Risa', 'Japanese', 'Female'),
('Sayuri', 'Japanese', 'Female'), ('Yuri', 'Japanese', 'Female')

-- RUSSIAN MALE NAMES (50+)
INSERT INTO [dbo].[BotNames] ([Name], [Language], [Gender]) VALUES
('Aleksandr', 'Russian', 'Male'), ('Boris', 'Russian', 'Male'), ('Dmitri', 'Russian', 'Male'),
('Evgeny', 'Russian', 'Male'), ('Fyodor', 'Russian', 'Male'), ('Grigory', 'Russian', 'Male'),
('Igor', 'Russian', 'Male'), ('Ivan', 'Russian', 'Male'), ('Kirill', 'Russian', 'Male'),
('Leonid', 'Russian', 'Male'), ('Maxim', 'Russian', 'Male'), ('Nikolai', 'Russian', 'Male'),
('Oleg', 'Russian', 'Male'), ('Pavel', 'Russian', 'Male'), ('Roman', 'Russian', 'Male'),
('Sergei', 'Russian', 'Male'), ('Timur', 'Russian', 'Male'), ('Vladimir', 'Russian', 'Male'),
('Yuri', 'Russian', 'Male'), ('Andrei', 'Russian', 'Male'), ('Bogdan', 'Russian', 'Male'),
('Constantin', 'Russian', 'Male'), ('Daniil', 'Russian', 'Male'), ('Eduard', 'Russian', 'Male'),
('Filip', 'Russian', 'Male'), ('Gleb', 'Russian', 'Male'), ('Ilya', 'Russian', 'Male'),
('Mikhail', 'Russian', 'Male'), ('Nikita', 'Russian', 'Male'), ('Pyotr', 'Russian', 'Male'),
('Ruslan', 'Russian', 'Male'), ('Stanislav', 'Russian', 'Male'), ('Vadim', 'Russian', 'Male'),
('Viktor', 'Russian', 'Male'), ('Yaroslav', 'Russian', 'Male'), ('Alexei', 'Russian', 'Male'),
('Artem', 'Russian', 'Male'), ('Denis', 'Russian', 'Male'), ('Egor', 'Russian', 'Male'),
('Georgi', 'Russian', 'Male'), ('Innokenti', 'Russian', 'Male'), ('Konstantin', 'Russian', 'Male'),
('Lev', 'Russian', 'Male'), ('Makar', 'Russian', 'Male'), ('Nikifor', 'Russian', 'Male'),
('Pasha', 'Russian', 'Male'), ('Rodion', 'Russian', 'Male'), ('Saveli', 'Russian', 'Male'),
('Taras', 'Russian', 'Male'), ('Valentin', 'Russian', 'Male'), ('Vsevolod', 'Russian', 'Male'),
('Zakhar', 'Russian', 'Male'), ('Anton', 'Russian', 'Male'), ('German', 'Russian', 'Male'),
('Platon', 'Russian', 'Male'), ('Semyon', 'Russian', 'Male')

-- RUSSIAN FEMALE NAMES (50+)
INSERT INTO [dbo].[BotNames] ([Name], [Language], [Gender]) VALUES
('Anastasia', 'Russian', 'Female'), ('Arina', 'Russian', 'Female'), ('Daria', 'Russian', 'Female'),
('Ekaterina', 'Russian', 'Female'), ('Galina', 'Russian', 'Female'), ('Irina', 'Russian', 'Female'),
('Karina', 'Russian', 'Female'), ('Larisa', 'Russian', 'Female'), ('Maria', 'Russian', 'Female'),
('Natalia', 'Russian', 'Female'), ('Olga', 'Russian', 'Female'), ('Polina', 'Russian', 'Female'),
('Sofia', 'Russian', 'Female'), ('Tatiana', 'Russian', 'Female'), ('Valentina', 'Russian', 'Female'),
('Yana', 'Russian', 'Female'), ('Alina', 'Russian', 'Female'), ('Diana', 'Russian', 'Female'),
('Elena', 'Russian', 'Female'), ('Kristina', 'Russian', 'Female'), ('Lyudmila', 'Russian', 'Female'),
('Marina', 'Russian', 'Female'), ('Nina', 'Russian', 'Female'), ('Svetlana', 'Russian', 'Female'),
('Veronika', 'Russian', 'Female'), ('Yulia', 'Russian', 'Female'), ('Alexandra', 'Russian', 'Female'),
('Angelina', 'Russian', 'Female'), ('Anna', 'Russian', 'Female'), ('Elizaveta', 'Russian', 'Female'),
('Ksenia', 'Russian', 'Female'), ('Milana', 'Russian', 'Female'), ('Nadezhda', 'Russian', 'Female'),
('Oksana', 'Russian', 'Female'), ('Raisa', 'Russian', 'Female'), ('Tamara', 'Russian', 'Female'),
('Ulyana', 'Russian', 'Female'), ('Vera', 'Russian', 'Female'), ('Zoya', 'Russian', 'Female'),
('Alla', 'Russian', 'Female'), ('Bella', 'Russian', 'Female'), ('Darya', 'Russian', 'Female'),
('Evgeniya', 'Russian', 'Female'), ('Inessa', 'Russian', 'Female'), ('Kira', 'Russian', 'Female'),
('Lada', 'Russian', 'Female'), ('Maya', 'Russian', 'Female'), ('Rada', 'Russian', 'Female'),
('Serafima', 'Russian', 'Female'), ('Vasilisa', 'Russian', 'Female'), ('Victoria', 'Russian', 'Female'),
('Zlata', 'Russian', 'Female'), ('Anfisa', 'Russian', 'Female'), ('Emilia', 'Russian', 'Female'),
('Margarita', 'Russian', 'Female'), ('Stanislava', 'Russian', 'Female')

-- GERMAN MALE NAMES (50+)
INSERT INTO [dbo].[BotNames] ([Name], [Language], [Gender]) VALUES
('Alexander', 'German', 'Male'), ('Benjamin', 'German', 'Male'), ('Christian', 'German', 'Male'),
('Daniel', 'German', 'Male'), ('Elias', 'German', 'Male'), ('Felix', 'German', 'Male'),
('Gustav', 'German', 'Male'), ('Heinrich', 'German', 'Male'), ('Jakob', 'German', 'Male'),
('Karl', 'German', 'Male'), ('Lukas', 'German', 'Male'), ('Maximilian', 'German', 'Male'),
('Noah', 'German', 'Male'), ('Oliver', 'German', 'Male'), ('Paul', 'German', 'Male'),
('Richard', 'German', 'Male'), ('Sebastian', 'German', 'Male'), ('Thomas', 'German', 'Male'),
('Viktor', 'German', 'Male'), ('Wilhelm', 'German', 'Male'), ('Anton', 'German', 'Male'),
('Bruno', 'German', 'Male'), ('Conrad', 'German', 'Male'), ('Dieter', 'German', 'Male'),
('Emil', 'German', 'Male'), ('Franz', 'German', 'Male'), ('Georg', 'German', 'Male'),
('Hans', 'German', 'Male'), ('Ignaz', 'German', 'Male'), ('Johann', 'German', 'Male'),
('Klaus', 'German', 'Male'), ('Ludwig', 'German', 'Male'), ('Martin', 'German', 'Male'),
('Norbert', 'German', 'Male'), ('Otto', 'German', 'Male'), ('Peter', 'German', 'Male'),
('Ralf', 'German', 'Male'), ('Stefan', 'German', 'Male'), ('Theodor', 'German', 'Male'),
('Ulrich', 'German', 'Male'), ('Valentin', 'German', 'Male'), ('Werner', 'German', 'Male'),
('Xaver', 'German', 'Male'), ('Zacharias', 'German', 'Male'), ('Albrecht', 'German', 'Male'),
('Bernhard', 'German', 'Male'), ('Clemens', 'German', 'Male'), ('Dominik', 'German', 'Male'),
('Erich', 'German', 'Male'), ('Friedrich', 'German', 'Male'), ('Gerhard', 'German', 'Male'),
('Hermann', 'German', 'Male'), ('Jonas', 'German', 'Male'), ('Leon', 'German', 'Male'),
('Matthias', 'German', 'Male'), ('Niklas', 'German', 'Male')

-- GERMAN FEMALE NAMES (50+)
INSERT INTO [dbo].[BotNames] ([Name], [Language], [Gender]) VALUES
('Anna', 'German', 'Female'), ('Brigitte', 'German', 'Female'), ('Charlotte', 'German', 'Female'),
('Dorothea', 'German', 'Female'), ('Elisabeth', 'German', 'Female'), ('Franziska', 'German', 'Female'),
('Greta', 'German', 'Female'), ('Helga', 'German', 'Female'), ('Ingrid', 'German', 'Female'),
('Johanna', 'German', 'Female'), ('Katharina', 'German', 'Female'), ('Lena', 'German', 'Female'),
('Maria', 'German', 'Female'), ('Natalie', 'German', 'Female'), ('Ottilie', 'German', 'Female'),
('Paula', 'German', 'Female'), ('Rosa', 'German', 'Female'), ('Sophia', 'German', 'Female'),
('Theresa', 'German', 'Female'), ('Ursula', 'German', 'Female'), ('Viktoria', 'German', 'Female'),
('Wilhelmine', 'German', 'Female'), ('Adelheid', 'German', 'Female'), ('Berta', 'German', 'Female'),
('Clara', 'German', 'Female'), ('Dora', 'German', 'Female'), ('Emma', 'German', 'Female'),
('Frieda', 'German', 'Female'), ('Gerda', 'German', 'Female'), ('Hanna', 'German', 'Female'),
('Ida', 'German', 'Female'), ('Julia', 'German', 'Female'), ('Klara', 'German', 'Female'),
('Laura', 'German', 'Female'), ('Martha', 'German', 'Female'), ('Nina', 'German', 'Female'),
('Olga', 'German', 'Female'), ('Petra', 'German', 'Female'), ('Ruth', 'German', 'Female'),
('Sabine', 'German', 'Female'), ('Tanja', 'German', 'Female'), ('Ute', 'German', 'Female'),
('Vera', 'German', 'Female'), ('Waltraud', 'German', 'Female'), ('Agnes', 'German', 'Female'),
('Beatrix', 'German', 'Female'), ('Cornelia', 'German', 'Female'), ('Diana', 'German', 'Female'),
('Elisa', 'German', 'Female'), ('Gabriele', 'German', 'Female'), ('Hedwig', 'German', 'Female'),
('Ilse', 'German', 'Female'), ('Jasmin', 'German', 'Female'), ('Lotte', 'German', 'Female'),
('Margarete', 'German', 'Female'), ('Nora', 'German', 'Female')

-- FRENCH MALE NAMES (50+)
INSERT INTO [dbo].[BotNames] ([Name], [Language], [Gender]) VALUES
('Alexandre', 'French', 'Male'), ('Bernard', 'French', 'Male'), ('Charles', 'French', 'Male'),
('Daniel', 'French', 'Male'), ('Etienne', 'French', 'Male'), ('Francois', 'French', 'Male'),
('Guillaume', 'French', 'Male'), ('Henri', 'French', 'Male'), ('Jacques', 'French', 'Male'),
('Laurent', 'French', 'Male'), ('Marc', 'French', 'Male'), ('Nicolas', 'French', 'Male'),
('Olivier', 'French', 'Male'), ('Pierre', 'French', 'Male'), ('Raphael', 'French', 'Male'),
('Sebastien', 'French', 'Male'), ('Thierry', 'French', 'Male'), ('Vincent', 'French', 'Male'),
('Antoine', 'French', 'Male'), ('Baptiste', 'French', 'Male'), ('Christophe', 'French', 'Male'),
('Denis', 'French', 'Male'), ('Eric', 'French', 'Male'), ('Fabien', 'French', 'Male'),
('Gabriel', 'French', 'Male'), ('Hugues', 'French', 'Male'), ('Jerome', 'French', 'Male'),
('Julien', 'French', 'Male'), ('Louis', 'French', 'Male'), ('Mathieu', 'French', 'Male'),
('Nathan', 'French', 'Male'), ('Pascal', 'French', 'Male'), ('Remy', 'French', 'Male'),
('Simon', 'French', 'Male'), ('Thomas', 'French', 'Male'), ('Valentin', 'French', 'Male'),
('Adrien', 'French', 'Male'), ('Bruno', 'French', 'Male'), ('Cedric', 'French', 'Male'),
('Damien', 'French', 'Male'), ('Emile', 'French', 'Male'), ('Felix', 'French', 'Male'),
('Georges', 'French', 'Male'), ('Hugo', 'French', 'Male'), ('Jean', 'French', 'Male'),
('Luc', 'French', 'Male'), ('Maurice', 'French', 'Male'), ('Noel', 'French', 'Male'),
('Philippe', 'French', 'Male'), ('Quentin', 'French', 'Male'), ('Romain', 'French', 'Male'),
('Sylvain', 'French', 'Male'), ('Theo', 'French', 'Male'), ('Victor', 'French', 'Male'),
('Yves', 'French', 'Male'), ('Alain', 'French', 'Male')

-- FRENCH FEMALE NAMES (50+)
INSERT INTO [dbo].[BotNames] ([Name], [Language], [Gender]) VALUES
('Amelie', 'French', 'Female'), ('Brigitte', 'French', 'Female'), ('Camille', 'French', 'Female'),
('Delphine', 'French', 'Female'), ('Emma', 'French', 'Female'), ('Francoise', 'French', 'Female'),
('Genevieve', 'French', 'Female'), ('Helene', 'French', 'Female'), ('Isabelle', 'French', 'Female'),
('Julie', 'French', 'Female'), ('Laure', 'French', 'Female'), ('Marie', 'French', 'Female'),
('Nathalie', 'French', 'Female'), ('Oceane', 'French', 'Female'), ('Pauline', 'French', 'Female'),
('Rose', 'French', 'Female'), ('Sophie', 'French', 'Female'), ('Valerie', 'French', 'Female'),
('Aurelie', 'French', 'Female'), ('Beatrice', 'French', 'Female'), ('Celine', 'French', 'Female'),
('Diane', 'French', 'Female'), ('Elise', 'French', 'Female'), ('Florence', 'French', 'Female'),
('Gabrielle', 'French', 'Female'), ('Juliette', 'French', 'Female'), ('Louise', 'French', 'Female'),
('Marguerite', 'French', 'Female'), ('Nadine', 'French', 'Female'), ('Odette', 'French', 'Female'),
('Pascale', 'French', 'Female'), ('Renee', 'French', 'Female'), ('Suzanne', 'French', 'Female'),
('Therese', 'French', 'Female'), ('Veronique', 'French', 'Female'), ('Yvonne', 'French', 'Female'),
('Adele', 'French', 'Female'), ('Bernadette', 'French', 'Female'), ('Caroline', 'French', 'Female'),
('Dominique', 'French', 'Female'), ('Estelle', 'French', 'Female'), ('Fanny', 'French', 'Female'),
('Giselle', 'French', 'Female'), ('Henriette', 'French', 'Female'), ('Irene', 'French', 'Female'),
('Jacqueline', 'French', 'Female'), ('Karine', 'French', 'Female'), ('Lea', 'French', 'Female'),
('Manon', 'French', 'Female'), ('Nicole', 'French', 'Female'), ('Patricia', 'French', 'Female'),
('Rosalie', 'French', 'Female'), ('Simone', 'French', 'Female'), ('Virginie', 'French', 'Female'),
('Zoe', 'French', 'Female'), ('Agathe', 'French', 'Female')

-- TURKISH MALE NAMES (50+)
INSERT INTO [dbo].[BotNames] ([Name], [Language], [Gender]) VALUES
('Ahmet', 'Turkish', 'Male'), ('Burak', 'Turkish', 'Male'), ('Can', 'Turkish', 'Male'),
('Deniz', 'Turkish', 'Male'), ('Emre', 'Turkish', 'Male'), ('Fatih', 'Turkish', 'Male'),
('Gokhan', 'Turkish', 'Male'), ('Hakan', 'Turkish', 'Male'), ('Ibrahim', 'Turkish', 'Male'),
('Kemal', 'Turkish', 'Male'), ('Mehmet', 'Turkish', 'Male'), ('Murat', 'Turkish', 'Male'),
('Onur', 'Turkish', 'Male'), ('Osman', 'Turkish', 'Male'), ('Serkan', 'Turkish', 'Male'),
('Taner', 'Turkish', 'Male'), ('Ugur', 'Turkish', 'Male'), ('Volkan', 'Turkish', 'Male'),
('Yusuf', 'Turkish', 'Male'), ('Ali', 'Turkish', 'Male'), ('Baris', 'Turkish', 'Male'),
('Cem', 'Turkish', 'Male'), ('Dogan', 'Turkish', 'Male'), ('Eren', 'Turkish', 'Male'),
('Furkan', 'Turkish', 'Male'), ('Halil', 'Turkish', 'Male'), ('Ismail', 'Turkish', 'Male'),
('Koray', 'Turkish', 'Male'), ('Levent', 'Turkish', 'Male'), ('Mustafa', 'Turkish', 'Male'),
('Omer', 'Turkish', 'Male'), ('Orhan', 'Turkish', 'Male'), ('Selim', 'Turkish', 'Male'),
('Tolga', 'Turkish', 'Male'), ('Umut', 'Turkish', 'Male'), ('Veli', 'Turkish', 'Male'),
('Yasin', 'Turkish', 'Male'), ('Zafer', 'Turkish', 'Male'), ('Alper', 'Turkish', 'Male'),
('Bilal', 'Turkish', 'Male'), ('Caner', 'Turkish', 'Male'), ('Doruk', 'Turkish', 'Male'),
('Efe', 'Turkish', 'Male'), ('Faruk', 'Turkish', 'Male'), ('Goksel', 'Turkish', 'Male'),
('Hasan', 'Turkish', 'Male'), ('Ilker', 'Turkish', 'Male'), ('Kaan', 'Turkish', 'Male'),
('Mert', 'Turkish', 'Male'), ('Nedim', 'Turkish', 'Male'), ('Oktay', 'Turkish', 'Male'),
('Ramazan', 'Turkish', 'Male'), ('Samet', 'Turkish', 'Male'), ('Tarik', 'Turkish', 'Male'),
('Ufuk', 'Turkish', 'Male'), ('Yigit', 'Turkish', 'Male')

-- TURKISH FEMALE NAMES (50+)
INSERT INTO [dbo].[BotNames] ([Name], [Language], [Gender]) VALUES
('Ayse', 'Turkish', 'Female'), ('Betul', 'Turkish', 'Female'), ('Canan', 'Turkish', 'Female'),
('Derya', 'Turkish', 'Female'), ('Elif', 'Turkish', 'Female'), ('Fatma', 'Turkish', 'Female'),
('Gamze', 'Turkish', 'Female'), ('Hatice', 'Turkish', 'Female'), ('Ipek', 'Turkish', 'Female'),
('Kadriye', 'Turkish', 'Female'), ('Leman', 'Turkish', 'Female'), ('Melis', 'Turkish', 'Female'),
('Nihal', 'Turkish', 'Female'), ('Ozge', 'Turkish', 'Female'), ('Pelin', 'Turkish', 'Female'),
('Rabia', 'Turkish', 'Female'), ('Selin', 'Turkish', 'Female'), ('Tugba', 'Turkish', 'Female'),
('Ummugulsum', 'Turkish', 'Female'), ('Vildan', 'Turkish', 'Female'), ('Yasemin', 'Turkish', 'Female'),
('Zeynep', 'Turkish', 'Female'), ('Asli', 'Turkish', 'Female'), ('Burcu', 'Turkish', 'Female'),
('Ceyda', 'Turkish', 'Female'), ('Dilek', 'Turkish', 'Female'), ('Emine', 'Turkish', 'Female'),
('Funda', 'Turkish', 'Female'), ('Gizem', 'Turkish', 'Female'), ('Hulya', 'Turkish', 'Female'),
('Irem', 'Turkish', 'Female'), ('Kubra', 'Turkish', 'Female'), ('Merve', 'Turkish', 'Female'),
('Neslihan', 'Turkish', 'Female'), ('Oznur', 'Turkish', 'Female'), ('Pinar', 'Turkish', 'Female'),
('Rumeysa', 'Turkish', 'Female'), ('Sevgi', 'Turkish', 'Female'), ('Tulay', 'Turkish', 'Female'),
('Umay', 'Turkish', 'Female'), ('Yildiz', 'Turkish', 'Female'), ('Zuhal', 'Turkish', 'Female'),
('Aysegul', 'Turkish', 'Female'), ('Berna', 'Turkish', 'Female'), ('Cansu', 'Turkish', 'Female'),
('Damla', 'Turkish', 'Female'), ('Ebru', 'Turkish', 'Female'), ('Filiz', 'Turkish', 'Female'),
('Gulden', 'Turkish', 'Female'), ('Hayriye', 'Turkish', 'Female'), ('Ilknur', 'Turkish', 'Female'),
('Kezban', 'Turkish', 'Female'), ('Leyla', 'Turkish', 'Female'), ('Meltem', 'Turkish', 'Female'),
('Nurcan', 'Turkish', 'Female'), ('Ozlem', 'Turkish', 'Female')

GO

PRINT 'Bot names populated successfully'
PRINT CAST((SELECT COUNT(*) FROM BotNames) AS VARCHAR) + ' names inserted'
GO
